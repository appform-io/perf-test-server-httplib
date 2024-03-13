FROM ubuntu:jammy

COPY testfiles testfiles
COPY test-server .
COPY entrypoint.sh .
EXPOSE 8000
CMD ["./entrypoint.sh"]
