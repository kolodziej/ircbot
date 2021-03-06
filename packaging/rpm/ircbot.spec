Name: ircbot
Summary: Extendable IRC bot written in C++
Version: 0.3.0
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
      -DBUILD_TESTS=OFF \
      -DGIT_COMMIT=%{GIT_COMMIT} -DGIT_REF=%{GIT_REF}
make -j $(nproc)

%install
make DESTDIR=$RPM_BUILD_ROOT install

%files
%{_bindir}/ircbot
%{_libdir}/ircbot/plugins/init.so
%{_libdir}/ircbot/plugins/join.so
%{_libdir}/ircbot/plugins/ping.so
%{_libdir}/ircbot/plugins/helloworld.so
%{_libdir}/ircbot/plugins/clipboard.so
%{_libdir}/ircbot/plugins/simple_commands.so
%{_libdir}/ircbot/plugins/uptime.so
%{_libdir}/libircbot.so
%{_libdir}/python2.7/site-packages/pyircbot.so
