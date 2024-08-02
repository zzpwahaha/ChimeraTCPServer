import socket
from time import sleep

def main():
    host = 'localhost' #'127.0.0.10'  # Server's IP address
    port = 8888        # Server's port number

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        a=0
        while True:
            # message = input("Enter message to send to server: ")
            # if message.lower() == 'exit':
            #     break
            message = f'test{a}'
            s.sendall(message.encode())
            data = s.recv(1024)
            print('Received from server:', data.decode())
            sleep(0.1)
            a+=1

if __name__ == "__main__":
    main()