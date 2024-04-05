# REST Server for Performance Testing
This project provides a simple REST based server for performance testing container infrastructure.

## Getting the container

The container is hosted on [ghcr.io](https://github.com/appform-io/perf-test-server-httplib/pkgs/container/perf-test-server-httplib).

Pull container using the following commands.

### Docker
```sh
$ docker pull ghcr.io/appform-io/perf-test-server-httplib
```
### Podman
```sh
$ podman pull ghcr.io/appform-io/perf-test-server-httplib
```

## Running the container
```
docker run -p 8000:8000 ghcr.io/appform-io/perf-test-server-httplib
```

To run on [podman](https://podman.io) replace `docker` with `podman` in the above command.

> To stop container, press `<Ctrl-C>`

## Usage
To access the container hit it using any load testing tool on port 8000 (or whatever host port you map the container port to).

### Sample `wrk` command
```shell
$ wrk -c 100 -t20 -d 30 --latency  http://localhost:8000
Running 30s test @ http://localhost:8000
  20 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   227.56ms  324.11ms   1.90s    79.61%
    Req/Sec     2.62k     2.20k   11.79k    63.51%
  Latency Distribution
     50%  440.00us
     75%  443.61ms
     90%  795.54ms
     99%    1.01s 
  1064444 requests in 30.02s, 106.39MB read
  Socket errors: connect 0, read 0, write 0, timeout 93
Requests/sec:  35453.26
Transfer/sec:      3.54MB
```
### Sample `ab` command
```shell
$ ab -k -t 30 -n 1000000 -c 20 http://localhost:8000/
This is ApacheBench, Version 2.3 <$Revision: 1879490 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 100000 requests
Completed 200000 requests
Completed 300000 requests
Completed 400000 requests
Completed 500000 requests
Completed 600000 requests
Completed 700000 requests
Completed 800000 requests
Completed 900000 requests
Completed 1000000 requests
Finished 1000000 requests


Server Software:        
Server Hostname:        localhost
Server Port:            8000

Document Path:          /
Document Length:        12 bytes

Concurrency Level:      20
Time taken for tests:   28.274 seconds
Complete requests:      1000000
Failed requests:        2
   (Connect: 0, Receive: 0, Length: 2, Exceptions: 0)
Keep-Alive requests:    800005
Total transferred:      104799863 bytes
HTML transferred:       11999976 bytes
Requests per second:    35368.31 [#/sec] (mean)
Time per request:       0.565 [ms] (mean)
Time per request:       0.028 [ms] (mean, across all concurrent requests)
Transfer rate:          3619.72 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   9.8      0    1031
Processing:     0    0   7.1      0    5001
Waiting:        0    0   0.2      0      15
Total:          0    0  12.1      0    5001

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      0
  75%      1
  80%      1
  90%      1
  95%      1
  98%      1
  99%      1
 100%   5001 (longest request)
```
## APIS
### Main API : `/`
This returns an output (Hello World!) in text format.
```shell
$ curl -vvv http://localhost:8000/
*   Trying 127.0.0.1:8000...
* Connected to localhost (127.0.0.1) port 8000 (#0)
> GET / HTTP/1.1
> Host: localhost:8000
> User-Agent: curl/7.81.0
> Accept: */*
> 
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Length: 12
< Content-Type: text/plain
< Keep-Alive: timeout=5, max=5
< 
* Connection #0 to host localhost left intact
Hello World!
```
### File download API: `/files/test.txt`
This returns a simple small file.
```shell
$ curl -vvv http://localhost:8000/files/test.txt
*   Trying 127.0.0.1:8000...
* Connected to localhost (127.0.0.1) port 8000 (#0)
> GET /files/test.txt HTTP/1.1
> Host: localhost:8000
> User-Agent: curl/7.81.0
> Accept: */*
> 
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Content-Length: 10
< Content-Type: text/plain
< Keep-Alive: timeout=5, max=5
< 
Test File
* Connection #0 to host localhost left intact
```

## Healthchecks
If configuring probes for checking controller health, `/` can be set as healthcheck and it will always return `HTTP 200/OK`. In case container needs to be made unhealthy, please point health check to `/unhealthy` API.

### Controlling behaviour of the `/unhealthy` API
By default, the API will return 200 OK 3 times and then it will start returning `HTTP 500 Server Error`.  To increase the count from 3 to any number, please pass an environment variable `HEALTHCOUNT` with the appropriate number.

```shell
$ curl  -I http://localhost:8000/unhealthy 2>&1 |grep HTTP
HTTP/1.1 200 OK
$ curl  -I http://localhost:8000/unhealthy 2>&1 |grep HTTP
HTTP/1.1 200 OK
$ curl  -I http://localhost:8000/unhealthy 2>&1 |grep HTTP
HTTP/1.1 200 OK
$ curl  -I http://localhost:8000/unhealthy 2>&1 |grep HTTP
HTTP/1.1 500 Internal Server Error
```

## Controlling behaviour using environment variables
The following environment variables  canb used to control different aspects of the container.
- `CORES` - By default the container detects the number of cores and creates a server thread for that. This can be controlled by setting the CORES variable to the required number. Usage: `-e "CORES=20"`
- `NOLOG` - Do not do access logging. Set this to stop console logging. This will significantly improve performance for the container. Usage: `-e "NOLOG=1"`
- `HEALTHCOUNT` - Number of counts after which the `/unhealthy` api starts returning `HTTP 500/Internal Server Error` instead of `HTTP 200/OK`. Usage: `-e "HEALTHCOUNT=20"`
- `DIE_FAST` - Exit the container after specified number of seconds. By default this is not set, i.e. container doesn;t exit by itself. Usage: `-e "DIE_FAST=5"`.

## Building
The server is written in C++. Code is hosted [here](https://github.com/appform-io/perf-test-server-httplib).

## Clone the code
```shell
$ git clone git@github.com:appform-io/perf-test-server-httplib.git
```
## Fetch dependencies
```shell
$ git submodule update --init --recursive
```
## Build
```shell
make
```

This will generate a binary called `test-server` in the current directory and a docker container. Tag and push the container as needed.

## License
Apache 2.0

## Base image
This container uses [Redhat Universal Base Image](https://catalog.redhat.com/software/base-images)
