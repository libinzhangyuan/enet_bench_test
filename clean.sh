OLD_PWD="$( pwd )"

cd ./enet_client/ && make clean && \
    cd ../enet_server/ && make clean && \
    echo ""
cd ../

# restore old path.
cd $OLD_PWD
