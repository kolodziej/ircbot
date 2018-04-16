Name: ircbot
Summary: Extendable IRC bot written in C++
Version: 0.2.0
Release: 1
License: GPL
Group: Applications/Internet
URL: https://kacperkolodziej.pl/ircbot.html
Vendor: Kacper Kołodziej <kacper@kolodziej.it>
Packager: Kacper Kołodziej <kacper@kolodziej.it>

%description
Extendable IRC bot written in C++

%build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/usr \
      -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_TESTS=OFF
make -j $(nproc)

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%{_bindir}/ircbot
%{_bindir}/ircbotctl
%{_libdir}/ircbot/plugins/init.so
%{_libdir}/ircbot/plugins/join.so
%{_libdir}/ircbot/plugins/ping.so
%{_libdir}/ircbot/plugins/helloworld.so
%{_libdir}/ircbot/plugins/clipboard.so
%{_libdir}/ircbot/plugins/simple_commands.so
%{_libdir}/libircbot.so
