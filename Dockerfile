FROM ubuntu

COPY * /home/default/calculator/
WORKDIR /home/default/calculator/
RUN make
RUN /home/default/calculator/calculator server 30
