echo "Generating MeepBot autotools files..."
autoheader
aclocal
autoconf
automake

echo "Generating sqlite3 autotools files..."
cd sqlite3
aclocal
autoconf
automake
cd ..

echo "Configuring and building."
./configure --enable-debug
make
