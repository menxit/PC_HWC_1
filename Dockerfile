FROM centos:7

RUN yum -y update && \
    yum groupinstall -y 'Development Tools' && \
    yum install -y wget cmake && \
    rpm -Uvh https://rpmfind.net/linux/fedora/linux/development/rawhide/Everything/x86_64/os/Packages/c/CUnit-2.1.3-16.fc27.x86_64.rpm && \
    cd /opt && \
    wget https://downloads.sourceforge.net/project/cunit/CUnit/2.1-3/CUnit-2.1-3.tar.bz2 && \
    tar xjf CUnit-2.1-3.tar.bz2 && \
    cd CUnit-2.1-3 && \
    autoconf && \
    autoreconf -i && \
    ./configure && \
    make && \
    make install

VOLUME ["/opt/code"]

WORKDIR "/opt/code"
