Name: ircbot
Summary: Extendable IRC bot written in C++
Version: 0.1.1
Release: 1
License: GPL
Group: Applications/Internet
URL: https://kacperkolodziej.pl/ircbot.html
Vendor: Kacper Kołodziej <kacper@kolodziej.it>
Packager: Kacper Kołodziej <kacper@kolodziej.it>

%description
Extendable IRC bot written in C++

%build
cmake -G "Unix Makefiles"
make

%install
make install

%files
%{_bindir}/ircbot
