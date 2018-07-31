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
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	cert   *string
	key    *string
	caCert *string

	kubeconfig *string

	ipEtcdCache = make(map[string]string)
)

const (
	updateTime           = 1 * time.Second
	requestTimeout       = 5 * time.Second
	dialKeepAliveTime    = 10 * time.Second
	dialKeepAliveTimeout = 3 * time.Second
	keyDir               = "Microsoft/FreeFlow/"
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

	caCert = flag.String("cacert", "Server's TLS CA Certificate", "etcd cacert")
	cert = flag.String("cert", "Client's SSL Certificate", "etcd cert")
	key = flag.String("key", "Client's SSL Private Key", "etcd key")

	endpoints := strings.Split(*flag.String("endpoints", os.Getenv("ETCD_ENDPOINTS"), "etcd endpoints"), ",")
	namespace := flag.String("namespace", "default", "name space to query")

	fmt.Println("endpoints: ", endpoints)
	flag.Parse()

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

	updateIP(namespace, clientset, endpoints)
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

func updateIP(namespace *string, clientset *kubernetes.Clientset, endpoints []string) {
	clientetcd := etcdClient(endpoints)
	defer clientetcd.Close()

	for true {
		// fetch pod information
		pods, err := clientset.CoreV1().Pods(*namespace).List(metav1.ListOptions{})
		if err != nil {
			panic(err.Error())
		}

		// update ip into new map
		ipChangedMap := make(map[string]string)
		for _, pod := range pods.Items {
			if hostOldIP, ok := ipEtcdCache[pod.Status.PodIP]; ok {
				if hostOldIP == pod.Status.HostIP {
					continue
				}
			}
			// fmt.Println(pod.Status.PodIP + " <- " + pod.Status.HostIP)
			ipEtcdCache[pod.Status.PodIP] = pod.Status.HostIP
			ipChangedMap[pod.Status.PodIP] = pod.Status.HostIP
		}

		// delete key-value ip map in etcd recursively
		if len(ipChangedMap) != 0 {
			for k, v := range ipChangedMap {
				_, err := clientetcd.Put(context.Background(), keyDir+k, v)
				if err != nil {
					panic(err.Error())
				}
			}
		}

		time.Sleep(updateTime)
	}

}
