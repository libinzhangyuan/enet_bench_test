OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH

# `rm asio_kcp_client/asio_kcp_client 2>/dev/null ; rm asio_kcp_server/asio_kcp_server 2>/dev/null`


echo "" && echo "" && echo "" && echo "" && echo ""
echo `date`
echo "============================================================"
echo "        Compiling enet_bench_test                     ====="
echo "========================================================="

echo "" && echo "" && echo "[-------------------------------]" && echo "   enet_client" && echo "[-------------------------------]" && \
    cd ./enet_client/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   enet_server" && echo "[-------------------------------]" && \
    cd ../enet_server/ && make && \
echo ""

# restore old path.
cd $OLD_PWD
