# ======================================================================
# This file is part of FurniOS, a small experimental operating system
# for Raspberry Pi.
#
# Copyright (C) 2026 Oliver Arendt (Furnio, Fkolleg)
# www.oliverarendt.de
#
# FurniOS is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# FurniOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see
# <https://www.gnu.org/licenses/>.
# ======================================================================

import os
import socket
import subprocess
import time

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


def assert_in_order(raw: bytes, tokens: list[bytes]) -> None:
    pos = 0
    for t in tokens:
        i = raw.find(t, pos)
        assert i != -1, f"Missing token {t!r} after index {pos}. Raw tail: {raw[pos:pos+200]!r}"
        pos = i + len(t)

