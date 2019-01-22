project('udpiq', 'c',
	default_options : [ 'warning_level=3',
                        'buildtype=debugoptimized' ],
	version : '0.1.0',
	license : 'MIT')

cc = meson.get_compiler('c')

# Optimizer arguments for Raspberry Pi 3b+
c_args = ['-mcpu=cortex-a53',
          '-mfpu=neon-fp-armv8',
          '-mfloat-abi=hard',
          '-funsafe-math-optimizations',]

m_dep = cc.find_library('m')
alsa_dep = dependency('alsa')

udpiq_sources = ['main.c', 'alsa.c',]

executable('udpiq',
    udpiq_sources,
    c_args: c_args,
    dependencies: [alsa_dep, m_dep,]
)
