FROM registry.access.redhat.com/ubi9/ubi-micro

COPY testfiles testfiles
COPY test-server .
COPY entrypoint.sh .
EXPOSE 8000
CMD ["./entrypoint.sh"]
