FROM ubuntu:16.04

# Setup environ
ENV user newbie
ENV prob_port 51235
ENV binary stackmachina_release

# Install packages
RUN apt-get update
RUN apt-get install -y socat

# Change permission
RUN chmod 1733 /tmp /var/tmp

# Additional configuration
RUN adduser --gecos "" --disabled-password $user
ADD ./$binary /home/$user/$binary
ADD ./flag /home/$user/flag

RUN chown -R root:root /home/$user/
RUN chown root:$user /home/$user/$binary
RUN chown root:$user /home/$user/flag

RUN chmod 2755 /home/$user/$binary
RUN chmod 440 /home/$user/flag

# final
CMD socat -T 30 TCP-LISTEN:$prob_port,reuseaddr,fork EXEC:/home/$user/$binary
USER $user
EXPOSE $prob_port

