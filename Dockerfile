FROM ubuntu

COPY * /home/default/calculator/
WORKDIR /home/default/calculator/
CMD ["calculator", "server", "30", "portHTTP", "8080"]
