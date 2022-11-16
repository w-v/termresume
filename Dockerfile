FROM linuxserver/openssh-server

ENV INSTALL_DIR=/srv/tres

RUN apk update && apk add musl-dev notcurses notcurses-dev notcurses-libs make gcc git

RUN echo "\
\
 ███████████            █████      ███                  █████████       █████  ███  ████   ███ \
▒▒███▒▒▒▒▒███          ▒▒███      ▒▒▒                  ███▒▒▒▒▒███     ▒▒███  ▒▒▒  ▒▒███  ▒▒▒  \
 ▒███    ▒███   ██████  ▒███████  ████  ████████      ▒███    ▒███   ███████  ████  ▒███  ████ \
 ▒██████████   ███▒▒███ ▒███▒▒███▒▒███ ▒▒███▒▒███     ▒███████████  ███▒▒███ ▒▒███  ▒███ ▒▒███ \
 ▒███▒▒▒▒▒███ ▒███ ▒███ ▒███ ▒███ ▒███  ▒███ ▒███     ▒███▒▒▒▒▒███ ▒███ ▒███  ▒███  ▒███  ▒███ \
 ▒███    ▒███ ▒███ ▒███ ▒███ ▒███ ▒███  ▒███ ▒███     ▒███    ▒███ ▒███ ▒███  ▒███  ▒███  ▒███ \
 █████   █████▒▒██████  ████████  █████ ████ █████    █████   █████▒▒████████ █████ █████ █████\
▒▒▒▒▒   ▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒ ▒▒▒▒▒    ▒▒▒▒▒   ▒▒▒▒▒  ▒▒▒▒▒▒▒▒ ▒▒▒▒▒ ▒▒▒▒▒ ▒▒▒▒▒ \
\
" > /etc/motd

COPY Makefile ${INSTALL_DIR}/
COPY include ${INSTALL_DIR}/include
COPY src ${INSTALL_DIR}/src
COPY data ${INSTALL_DIR}/data
RUN mkdir -pv ${INSTALL_DIR}/obj

RUN chown -R abc:abc ${INSTALL_DIR}

WORKDIR ${INSTALL_DIR}
# RUN make deps
# RUN make

# RUN usermod --shell ${INSTALL_DIR}/tres abc 
