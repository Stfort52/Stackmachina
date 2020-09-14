FROM ubuntu:16.04

# Setup environ
ENV user newbie
ENV prob_port 51236
ENV binary stackmachina_rev_release

# Install packages
RUN apt-get update
RUN apt-get install -y socat

# Change permission
RUN chmod 1733 /tmp /var/tmp

# Additional configuration
RUN adduser --gecos "" --disabled-password $user
ADD ./$binary /home/$user/$binary
ADD ./binary.sm /home/$user/binary.sm

RUN echo "HAHA THIS IS NOT A FLAG :D" > /home/$user/flag

RUN chown -R root:root /home/$user/
RUN chown root:$user /home/$user/$binary
RUN chown root:$user /home/$user/flag
RUN chown root:$user /home/$user/binary.sm

RUN chmod 755 /home/$user/$binary
RUN chmod 777 /home/$user/flag
RUN chmod 744 /home/$user/binary.sm
# final
CMD socat -T 30 TCP-LISTEN:$prob_port,reuseaddr,fork EXEC:"/home/$user/$binary /home/$user/binary.sm"
USER $user
EXPOSE $prob_port

