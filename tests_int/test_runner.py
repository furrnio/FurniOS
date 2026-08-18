import os
import socket
import subprocess
import time

from test_utils import recv_until_timeout
from test_utils import recv_until_term_or_timeout
from test_utils import assert_in_order 

KERNEL_ELF = "build/kernel.elf"
QEMU_CMD = [
    "qemu-system-arm",
    "-M", "raspi2b",
    "-m", "1024",
    "-nographic",
    "-icount", "shift=9",
    "-kernel", KERNEL_ELF,
    "-serial", "tcp:127.0.0.1:5556,server,nowait",
    "-display", "none",
    "-monitor", "none"
]

HOST = "127.0.0.1"
PORT = 5556


def run_test(test_input: str, timeout: num, terminator=""):
    proc = subprocess.Popen(QEMU_CMD, 
                            stdout=subprocess.DEVNULL, 
                            stderr=subprocess.PIPE, 
                            text=True)

    try:
        sock = None
        for _ in range(10): # Retry a few times
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(0.2)
                sock.connect((HOST, PORT))
                break
            except ConnectionRefusedError as e:
                time.sleep(0.1)

        if sock is None:
            raise RuntimeError(f"Could not connect to QEMU UART socket: {last_err}")

        # Now we are connected to UART, let's wait a bit

        time.sleep(0.1)

        print("===== SENDING =====")
        print(test_input)

        sock.sendall(test_input.encode("utf-8"))
    
        if terminator == "":
            raw = recv_until_timeout(sock, timeout)
        else:
            raw = recv_until_term_or_timeout(sock, terminator, timeout=5)

        print("===== RECEIVE =====")
        print(raw)

        return raw

    finally:
        try:
            if sock is not None:
                sock.close()
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                proc.kill()

def test_startup():
    expected = b"=== FurniOS booted ===\n"
    response = run_test("", 5.0, expected);
    assert raw == expected, f"Cannot find {expected}\n";

def test_uart():
    expected = b"Do data abort..."
    raw = run_test("a", 5.0, expected)
    assert expected in raw, f"Cannot find the string {expected}\n";

    expected = b"Do prefetch abort..."
    raw = run_test("p", 5.0, expected)
    assert expected in raw, f"Cannot find the string {expected}\n";


    expected = b"Do prefetch abort..."
    raw = run_test("ppp", 5.0)
    assert raw.count(expected) == 3
    
    expected = [
        b"Do data abort...\n",
        b"Do prefetch abort...\n",
        b"Do data abort...\n",
        b"Do prefetch abort...\n",
    ]
    raw = run_test("apap", timeout=5.0)  # raw is str
    
    assert_in_order(raw, expected)



if __name__ == "__main__":
    #test_startup()
    test_uart()
    #test_excection_handlers()
    #test_something
 
