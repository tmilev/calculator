FROM ubuntu

COPY * /home/default/calculator/
WORKDIR /home/default/calculator/
RUN /home/default/calculator/calculator server 30 portHTTP 8080
