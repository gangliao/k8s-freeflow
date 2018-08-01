// MIT License

// Copyright (c) 2018 Gang Liao <gangliao@cs.umd.edu>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

package main

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/coreos/etcd/clientv3"
	"k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	cert       *string
	key        *string
	caCert     *string
	kubeconfig *string
)

const (
	updateTime           = 1 * time.Second
	requestTimeout       = 5 * time.Second
	dialKeepAliveTime    = 10 * time.Second
	dialKeepAliveTimeout = 3 * time.Second
	ipMapKeyDir          = "Microsoft/FreeFlow/"
	ipNodesKeyDir        = "Kubernetes/Nodes/"
)

func homeDir() string {
	if h := os.Getenv("HOME"); h != "" {
		return h
	}
	return os.Getenv("USERPROFILE") // windows
}

func main() {
	if home := homeDir(); home != "" {
		kubeconfig = flag.String("kubeconfig", filepath.Join(home, ".kube", "config"), "(optional) absolute path to the kubeconfig file")
	} else {
		kubeconfig = flag.String("kubeconfig", "", "absolute path to the kubeconfig file")
	}

	caCert = flag.String("cacert", "/etc/kubernetes/ssl/ca.pem", "Server's TLS CA Certificate")
	cert = flag.String("cert", "", "Client's SSL Certificate")
	key = flag.String("key", "", "Client's SSL Private Key")

	epts := flag.String("endpoints", os.Getenv("ETCD_ENDPOINTS"), "etcd endpoints")
	namespace := flag.String("namespace", "default", "name space to query")

	flag.Parse()

	endpoints := strings.Split(*epts, ",")

	fmt.Println("kubeconfig: ", kubeconfig)
	fmt.Println("endpoints: ", endpoints)
	fmt.Println("namespace: ", namespace)
	fmt.Println("CA certificate: ", caCert)

	// use the current context in kubeconfig
	config, err := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	if err != nil {
		panic(err.Error())
	}

	// creates the clientset
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err.Error())
	}

	ip2Etcd(namespace, clientset, endpoints)
}

func etcdClient(endpoints []string) *clientv3.Client {
	cfg := clientv3.Config{
		Endpoints:            endpoints,
		DialTimeout:          requestTimeout,
		DialKeepAliveTime:    dialKeepAliveTime,
		DialKeepAliveTimeout: dialKeepAliveTimeout,
	}

	tlsEnabled := false
	tlsConfig := &tls.Config{
		InsecureSkipVerify: false,
	}

	if *caCert != "" {
		certBytes, err := ioutil.ReadFile(*caCert)
		if err != nil {
			panic(err.Error())
		}

		caCertPool := x509.NewCertPool()
		ok := caCertPool.AppendCertsFromPEM(certBytes)

		if ok {
			tlsConfig.RootCAs = caCertPool
		}
		tlsEnabled = true
	}

	if *cert != "" && *key != "" {
		tlsCert, err := tls.LoadX509KeyPair(*cert, *key)
		if err != nil {
			panic(err.Error())
		}
		tlsConfig.Certificates = []tls.Certificate{tlsCert}
		tlsEnabled = true
	}

	if tlsEnabled {
		cfg.TLS = tlsConfig
	}

	clientetcd, err := clientv3.New(cfg)
	if err != nil {
		panic(err.Error())
	}

	return clientetcd
}

func ipMap2Etcd(clientetcd *clientv3.Client, pods *v1.PodList, ipPrecedeMap *map[string]string) {
	// ipCurrentMap: whole IP map during the cur round
	ipCurrentMap := make(map[string]string)
	// ipChangedMap: changed IP map between the prev and cur round
	ipChangedMap := make(map[string]string)

	for _, pod := range pods.Items {
		if len(pod.Status.PodIP) == 0 || len(pod.Status.HostIP) == 0 {
			continue
		}
		ipCurrentMap[pod.Status.PodIP] = pod.Status.HostIP
		if precedeHostIP, ok := (*ipPrecedeMap)[pod.Status.PodIP]; ok {
			if precedeHostIP == pod.Status.HostIP {
				continue
			}
		}
		ipChangedMap[pod.Status.PodIP] = pod.Status.HostIP
	}

	// Add changed IP into ETCD
	for k, v := range ipChangedMap {
		_, err := clientetcd.Put(context.Background(), ipMapKeyDir+k, v)
		if err != nil {
			panic(err.Error())
		}
	}

	// Delete expired IP from ETCD
	// Memory Optimization on server side which on watch mode can capture this signal
	// and delete the expired ip entries.
	for k := range *ipPrecedeMap {
		if _, ok := ipCurrentMap[k]; !ok {
			_, err := clientetcd.Delete(context.Background(), ipMapKeyDir+k)
			if err != nil {
				panic(err.Error())
			}
		}
	}

	*ipPrecedeMap = ipCurrentMap
}

func ipNode2Etcd(clientetcd *clientv3.Client, nodes *v1.NodeList, ipPrecedeNodes *map[string]string) {
	// ipCurrentNodes: whole IP Nodes during the cur round
	ipCurrentNodes := make(map[string]string)
	// ipChangedNodes: changed IP Nodes between the prev and cur round
	ipChangedNodes := make(map[string]string)

	var internIP string
	var hostName string
	for _, node := range nodes.Items {
		for _, a := range node.Status.Addresses {
			if a.Type == v1.NodeInternalIP {
				internIP = a.Address
			} else if a.Type == v1.NodeHostName {
				hostName = a.Address
			}
		}

		if len(internIP) == 0 {
			continue
		}

		ipCurrentNodes[internIP] = hostName
		if precedeHostName, ok := (*ipPrecedeNodes)[internIP]; ok {
			if precedeHostName == hostName {
				continue
			}
		}
		ipChangedNodes[internIP] = hostName
	}

	// Add changed IP Nodes into ETCD
	for k, v := range ipChangedNodes {
		_, err := clientetcd.Put(context.Background(), ipNodesKeyDir+k, v)
		if err != nil {
			panic(err.Error())
		}
	}

	// Delete expired Node IP from ETCD
	// Memory Optimization on server side which on watch mode can capture this signal
	// and delete the expired ip entries.
	for k := range *ipPrecedeNodes {
		if _, ok := ipCurrentNodes[k]; !ok {
			_, err := clientetcd.Delete(context.Background(), ipNodesKeyDir+k)
			if err != nil {
				panic(err.Error())
			}
		}
	}

	*ipPrecedeNodes = ipCurrentNodes
}

func ip2Etcd(namespace *string, clientset *kubernetes.Clientset, endpoints []string) {
	clientetcd := etcdClient(endpoints)
	defer clientetcd.Close()

	// clean etcd ip map after restarting the program
	_, err := clientetcd.Delete(context.Background(), ipMapKeyDir, clientv3.WithPrefix())
	if err != nil {
		panic(err.Error())
	}
	_, err = clientetcd.Delete(context.Background(), ipNodesKeyDir, clientv3.WithPrefix())
	if err != nil {
		panic(err.Error())
	}

	// ipPrecedeMap: whole IP map during the prev round
	ipPrecedeNodes := make(map[string]string)
	ipPrecedeMap := make(map[string]string)
	for true {
		// Fetch pod information
		pods, err := clientset.CoreV1().Pods(*namespace).List(metav1.ListOptions{})
		if err != nil {
			panic(err.Error())
		}
		ipMap2Etcd(clientetcd, pods, &ipPrecedeMap)

		// Fetch node information
		nodes, err := clientset.CoreV1().Nodes().List(metav1.ListOptions{})
		if err != nil {
			panic(err.Error())
		}
		ipNode2Etcd(clientetcd, nodes, &ipPrecedeNodes)

		time.Sleep(updateTime)
	}
}
