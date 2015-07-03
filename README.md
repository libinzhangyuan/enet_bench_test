
#### Compile:
1. enet<br>
  download enet 1.3.13 from: http://enet.bespin.org/download/enet-1.3.13.tar.gz <br>
         or just use the pack in install_pack folder. <br>
  unpack it.   $ tar zxvf ./enet-1.3.13.tar.gz <br>
  cd enet-1.3.13 <br>
  ./configure && make <br>

2. run  sh allmake.sh<br>

#### Run test
* ./enet_server/enet_server 0.0.0.0 22222
* ./enet_client/enet_client 120.26.200.xxx 22222 500

#### Bench mark
You can see the bench mark at [here](https://github.com/libinzhangyuan/reliable_udp_bench_mark)
