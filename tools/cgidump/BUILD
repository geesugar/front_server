cc_library(
    name = 'parse_pkg',
    srcs = [
        'parse_pkg.cpp',
        'utils.cpp',
        'tcp_buf.cpp',
        'http_buf.cpp',
        'session.cpp',
    ],
    deps = [
        '//thirdparty/libpcap:pcap',
    ],
    export_incs = [
        '/home/longtao/work/front_server/thirdparty/libpcap/include',
    ],
    defs = [],
    optimize = ['O0'],
    extra_cppflags = [],
)

cc_binary(
    name = 'cgidump',
    srcs = [
        'main.cpp',
    ],
    deps = [
        '//thirdparty/libpcap:pcap',
        ':parse_pkg',
    ],
    export_incs = [
        '/home/longtao/work/front_server/thirdparty/libpcap/include',
    ],
    defs = [],
    optimize = ['O0'],
    extra_cppflags = [],
)

cc_binary(
    name = 'mock',
    srcs = [
        'mock.cpp',
    ],
    deps = [
        '//thirdparty/libpcap:pcap',
        ':parse_pkg',
    ],
    export_incs = [
        '/home/longtao/work/front_server/thirdparty/libpcap/include',
    ],
    defs = [],
    optimize = ['O0'],
    extra_cppflags = [],
)

cc_binary(
    name = 'test_tcp_buf',
    srcs = [
        'test/test_tcp.cpp',
    ],
    deps = [
        ':parse_pkg',
        '#pthread',
        '#rt',
    ],
    export_incs = [
        '/home/longtao/work/front_server/thirdparty/libpcap/include',
    ],
    defs = [],
    optimize = ['O0'],
    extra_cppflags = [],
  )
