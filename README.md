# function
- socket IO
  - epoll: I/O duolufuyong; ET+ONESHOT; 
  - xianchengchi: reactor gaobingfa moxing
  - xianchengtongbu: shengchanzhexiaofeizhemoxing
  - heap: guanbichaoshi
- http
  - STL vector request/response buffer
  - zhuangtaiji: http jiexi
  - ziyuanqingqiu
- log
  - yibu
  - danlimoshi
  - zhuseduilie

# structure

- src
  - client.cpp
    - http client
    - handle fd: init, close, read, write
  - http.cpp
    - handle request and response
  - epoll.cpp
    - handle fd set
  - process.cpp
    - handle server function
  - server.cpp
    - integrate these modules with thread_pool.h


# log
- need to close connection after write
- ONESHOT remember to re-flag for listen_fd