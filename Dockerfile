FROM linuxserver/openssh-server

ENV INSTALL_DIR=/srv/tres

RUN apk update && apk add musl-dev notcurses notcurses-dev notcurses-libs make gcc git


COPY Makefile ${INSTALL_DIR}/
COPY include ${INSTALL_DIR}/include
COPY src ${INSTALL_DIR}/src
RUN mkdir ${INSTALL_DIR}/obj

RUN chown -R abc:abc ${INSTALL_DIR}

WORKDIR ${INSTALL_DIR}
# RUN make deps
# RUN make clean
# RUN make

# RUN usermod --shell ${INSTALL_DIR}/tres abc 
