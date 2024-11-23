
echo "Clone prometheus-client-c repository"
git clone https://github.com/digitalocean/prometheus-client-c.git

echo "Fix incompatible function pointer in promhttp.c"
sed -i 's/&promhttp_handler/(MHD_AccessHandlerCallback)promhttp_handler/' prometheus-client-c/promhttp/src/promhttp.c

echo "Build prom library"
mkdir -p prometheus-client-c/buildprom
cd prometheus-client-c/buildprom
cmake ../prom
make
sudo make install
cd ..
cd ..

echo "Build promhttp library"
mkdir -p prometheus-client-c/buildpromhttp
cd prometheus-client-c/buildpromhttp
cmake ../promhttp
make
sudo make install
cd ..
cd ..

echo "Build project"
python3 -m venv venv
source venv/bin/activate
pip install conan
mkdir build 
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
