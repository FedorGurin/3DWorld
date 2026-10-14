#!/usr/bin/env python3
"""Тестовый источник данных для 3DWorld.

Отправляет по UDP (порт 7030) пакеты PARAM_OBJ с несколькими объектами,
летающими по окружностям, и пакет LIST_VECTOR с ломаной линией.
Формат пакетов соответствует структурам из visudp.h и globalData.h
(x86/x64, выравнивание по умолчанию).

Пример:
    python3 tools/udp_test_sender.py --objects 3 --rate 20
"""
import argparse
import math
import socket
import struct
import time

PARAM_OBJ = 0
COMMAND = 2
LIST_VECTOR = 5
TC_RESET = 0

SIZE_BUF_DEFAULT = 2048
# THRequest: id, typeRequest, size, tics, dtTics
HEAD_FMT = "<5I"
# TMRequest после заголовка: compressed(u8), err(u8), 2 байта выравнивания, sizeBuf(u32)
MREQ_FMT = "<BBxxI"
HEAD_SIZE = struct.calcsize(HEAD_FMT) + struct.calcsize(MREQ_FMT)

# TSendVehicleVisSimple: id, code, 28 x double
VEHICLE_FMT = "<ii28d"


def make_packet(type_request, payload):
    size = HEAD_SIZE + len(payload)
    head = struct.pack(HEAD_FMT, 0, type_request, size, 0, 0)
    head += struct.pack(MREQ_FMT, 0, 0, len(payload))
    return head + payload


def vehicle(obj_id, code, t, x, y, z, psi, gamma=0.0, tan=0.0, v=200.0, alfa=0.0):
    # скорость в связанной СК с учетом угла атаки
    vcx = v * math.cos(alfa)
    vcy = -v * math.sin(alfa)
    # порядок полей как в TSendVehicleVisSimple
    fields = [t, x, y, z,          # time, cg_x, cg_y, cg_z
              psi, gamma, tan,     # psi, gamma, tan
              alfa, 0.0,           # alfa, beta
              0.0, 1.0, 0.0,       # nx, ny, nz
              v,                   # vc
              0.0, 0.0,            # fi, unt
              0.0, 0.0,            # fi0_geo, lam0_geo
              0.0, 0.0,            # fi_geo, lam_geo (0 - используются cg_x/cg_z)
              0.0, 0.0, 0.0,       # vg_x, vg_y, vg_z
              vcx, vcy, 0.0,       # vc_x, vc_y, vc_z
              0.0, 0.0, 0.0]       # omega_x, omega_y, omega_z
    return struct.pack(VEHICLE_FMT, obj_id, code, *fields)


def vector_list(points):
    # TSendListVectorsVisSimple: id, code, time, num(u8), 3 байта выравнивания, vec3[]
    payload = struct.pack("<iidB3x", 0, 0, 0.0, len(points))
    for p in points:
        payload += struct.pack("<3f", *p)
    return payload


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=7030)
    parser.add_argument("--objects", type=int, default=3)
    parser.add_argument("--rate", type=float, default=20.0, help="частота, Гц")
    parser.add_argument("--duration", type=float, default=0.0,
                        help="длительность, сек (0 - бесконечно)")
    parser.add_argument("--codes", default="101,102,105",
                        help="коды 3D моделей объектов через запятую")
    parser.add_argument("--line", action="store_true",
                        help="прямолинейный полет на север (проверка подгрузки рельефа)")
    parser.add_argument("--speed", type=float, default=250.0,
                        help="скорость прямолинейного полета, м/с")
    parser.add_argument("--reset", action="store_true",
                        help="отправить команду TC_RESET и выйти")
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = (args.host, args.port)

    if args.reset:
        packet = struct.pack(HEAD_FMT, 0, COMMAND, 24, 0, 0) + struct.pack("<i", TC_RESET)
        sock.sendto(packet, addr)
        return

    codes = [int(c) for c in args.codes.split(",")]
    t0 = time.time()
    dt = 1.0 / args.rate
    while True:
        t = time.time() - t0
        if args.duration and t > args.duration:
            break
        if args.line:
            # объекты летят на север друг за другом
            for i in range(args.objects):
                x = args.speed * t - 1500.0 * i
                payload = vehicle(i, codes[i % len(codes)], t, x, 6500.0, 300.0 * i,
                                  0.0, 0.0, 0.0, v=args.speed, alfa=math.radians(3.0))
                sock.sendto(make_packet(PARAM_OBJ, payload), addr)
            time.sleep(dt)
            continue
        for i in range(args.objects):
            r = 3000.0 + 1500.0 * i
            w = 200.0 / r
            a = w * t + i * 2.0 * math.pi / max(args.objects, 1)
            x = r * math.cos(a)
            z = r * math.sin(a)
            y = 6500.0 + 300.0 * i
            # курс по касательной к окружности
            psi = -a - math.pi / 2.0
            gamma = -0.3
            # тангаж плавно меняется, угол атаки 4 град
            tan = 0.15 * math.sin(0.3 * t)
            payload = vehicle(i, codes[i % len(codes)], t, x, y, z, psi, gamma, tan,
                              alfa=math.radians(4.0))
            sock.sendto(make_packet(PARAM_OBJ, payload), addr)
        # треугольник из 3D линий
        tri = [(0.0, 6000.0, 0.0), (2000.0, 7000.0, 0.0),
               (0.0, 7000.0, 2000.0), (0.0, 6000.0, 0.0)]
        sock.sendto(make_packet(LIST_VECTOR, vector_list(tri)), addr)
        time.sleep(dt)


if __name__ == "__main__":
    main()
