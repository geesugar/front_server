# front_server
high performance linux front server. developed by c++

# prepare
 - [Install blade](https://github.com/chen3feng/typhoon-blade)
 - sudo yum install flex
 - sudo yum install bison
 - sudo yum install cabal-dev
 - sudo yum libstdc++-static

# thridparty
 - glog-0.3.5 (installed gflags-2.2.1 develop version in system before compiling glog library, otherwise FLAGS_log_dir, FLAGS_v configured in flagfile would not take effect)
 - gflags-2.2.1 (compiled with cmake)
 - libevent-2.1.8-stable


# technology
[socket SO_REUSEADDR option](http://www.cnblogs.com/mydomain/archive/2011/08/23/2150567.html)

# appended
compile glog-0.3.5 occured error:
relocation R_X86_64_32S against `.rodata' can not be used when making a shared object; recompile with -fPIC
