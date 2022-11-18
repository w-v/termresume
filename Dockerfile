FROM linuxserver/openssh-server

ENV TRES_INSTALL_DIR=/srv/tres

RUN apk update && apk add musl-dev notcurses notcurses-dev notcurses-libs make gcc git ncurses-terminfo-base

# RUN echo -e "\
#  ███████████            █████      ███                  █████████       █████  ███  ████   ███ \n\
# ▒▒███▒▒▒▒▒███          ▒▒███      ▒▒▒                  ███▒▒▒▒▒███     ▒▒███  ▒▒▒  ▒▒███  ▒▒▒  \n\
#  ▒███    ▒███   ██████  ▒███████  ████  ████████      ▒███    ▒███   ███████  ████  ▒███  ████ \n\
#  ▒██████████   ███▒▒███ ▒███▒▒███▒▒███ ▒▒███▒▒███     ▒███████████  ███▒▒███ ▒▒███  ▒███ ▒▒███ \n\
#  ▒███▒▒▒▒▒███ ▒███ ▒███ ▒███ ▒███ ▒███  ▒███ ▒███     ▒███▒▒▒▒▒███ ▒███ ▒███  ▒███  ▒███  ▒███ \n\
#  ▒███    ▒███ ▒███ ▒███ ▒███ ▒███ ▒███  ▒███ ▒███     ▒███    ▒███ ▒███ ▒███  ▒███  ▒███  ▒███ \n\
#  █████   █████▒▒██████  ████████  █████ ████ █████    █████   █████▒▒████████ █████ █████ █████\n\
# ▒▒▒▒▒   ▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒ ▒▒▒▒▒    ▒▒▒▒▒   ▒▒▒▒▒  ▒▒▒▒▒▒▒▒ ▒▒▒▒▒ ▒▒▒▒▒ ▒▒▒▒▒ \n\
# " > /etc/motd

COPY Makefile ${TRES_INSTALL_DIR}/
COPY include ${TRES_INSTALL_DIR}/include
COPY src ${TRES_INSTALL_DIR}/src
COPY data ${TRES_INSTALL_DIR}/data
RUN mkdir -pv ${TRES_INSTALL_DIR}/obj

RUN chown -R abc:abc ${TRES_INSTALL_DIR}

WORKDIR ${TRES_INSTALL_DIR}
RUN make deps
RUN make

# RUN usermod --shell ${TRES_INSTALL_DIR}/tres abc 
