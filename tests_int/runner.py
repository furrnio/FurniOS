import os
import socket
import subprocess
import time

KERNEL_ELF = "/Users/tabea.cubelic/Documents/Oliver/B_Folio/01_Spielwiese/_repos/furniOS/build/kernel.elf"
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

GOLDEN = "=== FURNIOS BOOTED ===\nREADY\nOK\nTEST_DONE\n"

def wait_for_bytes(sock, needle: bytes, timeout=10.0) -> bytes:
    deadline = time.time() + timeout
    buf = b""
    while time.time() < deadline:
        sock.settimeout(0.2)
        try:
            chunk = sock.recv(4096)
        except TimeoutError:
            continue
        if not chunk:
            continue
        buf += chunk
        if needle in buf:
            return buf
    raise TimeoutError(f"Timeout waiting for {needle!r}. Got: {buf[-200:]!r}")

def recv_until_timeout(sock, timeout: float, bufsize: int = 4096) -> bytes:
    deadline = time.time() + timeout
    buf = b""
    while time.time() < deadline:
        remaining = max(0.0, deadline - time.time())
        sock.settimeout(min(0.2, remaining))  # small polling chunks

        try:
            chunk = sock.recv(bufsize)
            if not chunk:
                continue
            buf += chunk
        except socket.timeout:
            pass
    return buf

def recv_until_term_or_timeout(sock, terminator: bytes, timeout: float, bufsize: int = 4096) -> bytes:
    deadline = time.time() + timeout
    buf = b""
    while time.time() < deadline:
        remaining = max(0.0, deadline - time.time())
        sock.settimeout(min(0.2, remaining))  # small polling chunks

        try:
            chunk = sock.recv(bufsize)
            if not chunk:
                continue
            buf += chunk
            if terminator in buf:
                break
        except socket.timeout:
            pass
    return buf

def run_test_old(test_input: str):
    proc = subprocess.Popen(QEMU_CMD, 
                            stdout=subprocess.DEVNULL, 
                            stderr=subprocess.PIPE, 
                            text=True)


    try:
        # retry connect a few times
        last_err = None
        sock = None
        for _ in range(50):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(0.2)
                sock.connect((HOST, PORT))
                break
            except ConnectionRefusedError as e:
                last_err = e
                time.sleep(0.1)
            except Exception as e:
                last_err = e
                time.sleep(0.1)

        if sock is None:
            raise RuntimeError(f"Could not connect to QEMU UART socket: {last_err}")

        # (Optional) wait for READY if your OS prints it
        # raw = recv_until(sock, b"READY\n", timeout=20)

        print("Sending:", test_input)
        sock.sendall(test_input.encode("utf-8") + b"\n")

        raw = recv_until(sock, b"TEST_DONE\n", timeout=5)
        return raw.decode("utf-8", errors="replace")

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

def test_integration():
    print("Run test...");
    output = run_test("run_something")
    assert output == GOLDEN, f"Mismatch!\nExpected:\n{GOLDEN}\nGot:\n{output}"

def run_test_uart(test_input: str):
    proc = subprocess.Popen(QEMU_CMD, 
                            stdout=subprocess.DEVNULL, 
                            stderr=subprocess.PIPE, 
                            text=True)

    try:
        # retry connect a few times
        sock = None
        for _ in range(10):
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(0.2)
                sock.connect((HOST, PORT))
                break
            except ConnectionRefusedError as e:
                time.sleep(0.1)

        if sock is None:
            raise RuntimeError(f"Could not connect to QEMU UART socket: {last_err}")

        print("Now we are connected to uart...")

        print("===== SENDING =====")
        print(test_input)

        sock.sendall(test_input.encode("utf-8"))

        #raw = recv_until_term_or_timeout(sock, b"TEST_DONE\n", timeout=5)
        raw = recv_until_timeout(sock, timeout=5)

        print("===== RECEIVE =====")
        print(raw)

        return raw.decode("utf-8", errors="replace")

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

def test_integration():
    print("Run test...");
    output = run_test_uart("asdf")
    #assert output == GOLDEN, f"Mismatch!\nExpected:\n{GOLDEN}\nGot:\n{output}"

if __name__ == "__main__":
    test_integration()
 
