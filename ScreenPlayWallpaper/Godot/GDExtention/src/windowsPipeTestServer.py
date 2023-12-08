# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
# python -m pip install pywin32
import win32pipe
import win32file
import pywintypes
import time

PIPE_NAME = 'ScreenPlay'

def create_pipe():
    return win32pipe.CreateNamedPipe(
        r'\\.\pipe\{}'.format(PIPE_NAME),
        win32pipe.PIPE_ACCESS_DUPLEX,
        win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_WAIT,
        1,  # max instances
        65536,  # out buffer size
        65536,  # in buffer size
        0,  # default timeout
        None  # security attributes
    )

def main():
    print("Starting Python Pipe Server...")
    pipe_handle = create_pipe()
    print("Waiting for a client to connect...")
    win32pipe.ConnectNamedPipe(pipe_handle, None)

    while True:
        try:
            # Reading from the client
            result, data = win32file.ReadFile(pipe_handle, 4096)
            if result == 0:  # 0 means the read operation completed successfully
                print("Received:", data.decode())
                
                # Sending a response back to the client
                response = "pong;"
                win32file.WriteFile(pipe_handle, response.encode())

        except pywintypes.error as e:
            if e.args[0] == 109:  # ERROR_BROKEN_PIPE
                print("Client disconnected.")
                # Disconnect and close the current pipe handle
                win32pipe.DisconnectNamedPipe(pipe_handle)
                win32file.CloseHandle(pipe_handle)

                # Create a new pipe instance for the next client
                pipe_handle = create_pipe()

            else:
                print("Error occurred:", e)


        #time.sleep(0.01)

if __name__ == '__main__':
    main()
