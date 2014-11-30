The C driver to Kafka

mkdir -p ${HOME}/projects
cd ${HOME}/projects
git clone https://github.com/edenhill/librdkafka.git
# sudo ./configure

The C driver makes use of the following Z utility

mkdir -p ${HOME}/projects/zlib
cd ${HOME}/projects/zlib
wget http://zlib.net/zlib-1.2.8.tar.gz
cd ${HOME}/projects/zlib/zlib-1.2.8 (or just cd zlib-1.2.8)
sudo ./configure
sudo make
sudo make install

