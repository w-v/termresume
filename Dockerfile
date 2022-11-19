FROM linuxserver/openssh-server

ENV TRES_INSTALL_DIR=/srv/tres

RUN apk update && apk add musl-dev notcurses notcurses-dev notcurses-libs make gcc git ncurses-terminfo-base

COPY data/motd /etc/motd

COPY Makefile ${TRES_INSTALL_DIR}/
COPY include ${TRES_INSTALL_DIR}/include
COPY src ${TRES_INSTALL_DIR}/src
COPY data ${TRES_INSTALL_DIR}/data
RUN mkdir -pv ${TRES_INSTALL_DIR}/obj

RUN chown -R abc:abc ${TRES_INSTALL_DIR}

WORKDIR ${TRES_INSTALL_DIR}
RUN make deps
RUN make

RUN usermod --shell ${TRES_INSTALL_DIR}/tres abc 
RUN echo -e "\
DisableForwarding yes\n\
AllowUsers en\n\
AuthenticationMethods password\n\
Banner ${TRES_INSTALL_DIR}/data/banner\n\
MaxSessions 1\n\
PermitRootLogin no\n\
PermitUserRC no\n\
PrintMotd yes\n\
SetEnv TERM_INSTALL_DIR=${TRES_INSTALL_DIR}\n\
ForceCommand en\n\
Match Address 172.16.0.2\n\
\tBanner none\n\
" >> /etc/ssh/sshd_config
