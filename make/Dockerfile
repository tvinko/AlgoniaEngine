FROM python:3.8.6-slim-buster

WORKDIR /home/algonia

RUN useradd docker

COPY ./out .

RUN cp libZenCommon.so /usr/lib && cp libZenCoreCLR.so /usr/lib
RUN apt-get update && apt-get install -y --no-install-recommends libgdiplus && apt-get install -y libgomp1
ENV DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=true

RUN chmod 755 /home/algonia/AlgoniaEngine
RUN chown -R docker:docker /home/algonia/templates
RUN chown -R docker:docker /home/algonia/tmp
RUN mkdir /home/docker
RUN chown -R docker:docker /home/docker
USER docker


CMD ["sh", "-c", "/home/algonia/templates/${TEMPLATE_NAME}/start.sh ${TEMPLATE_NAME}"]