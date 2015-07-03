This is a bench mark test for enet 1.3.13. <br>
enet: Reliable UDP networking library [website](http://enet.bespin.org/Features.html) <br>
It's also a example of enet server. <br>

#### Compile:
1. making gcc version >= 4.8 <br>

2. enet<br>
  download enet 1.3.13 from: http://enet.bespin.org/download/enet-1.3.13.tar.gz <br>
         or just use the pack in install_pack folder. <br>
  unpack it.   $ tar zxvf ./enet-1.3.13.tar.gz <br>
  cd enet-1.3.13 <br>
  ./configure && make <br>

3. run  sh allmake.sh<br>

#### Run test
* ./enet_server/enet_server 0.0.0.0 22222
* ./enet_client/enet_client 120.26.200.xxx 22222 500


#### Bench mark
You can see the bench mark at [here](https://github.com/libinzhangyuan/reliable_udp_bench_mark)
