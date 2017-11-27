from setuptools import setup, Extension

ext_module = Extension('utf8_escape', 
    sources = ['utf8_escape.cpp'], 
)

setup(
    name = 'utf8_escape', 
    version = '1.4',
    ext_modules = [ext_module],

    license = "BSD",
    url = "https://github.com/iljau/utf8_escape",
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Topic :: Text Processing :: Markup :: HTML',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python :: 2.7',
    ],
)
