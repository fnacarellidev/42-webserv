# Hello Webserv

## Types of Configuration

Configuration file setup can have 4 types of entry:

- One value, they represent only one value.
```
item value;
```

- Multi value, they represent a list of values separated by `,`.
```
item value1,value2,value3,...,valueN;
```

- One value map, they represent a key value pair.
```
item key=value;
```

- Multi value map, they represent a list of key value pairs separated by `,`.
```
item key1=value1,key2=value2,key3=value3,...,keyN=valueN;
```

## Server Block

The config file can have N server blocks, but they represent the highest level of the config file.   
Inside of them you can have:

- One value ***port***, above 1023 and below 65536.
```
port 4321;
```

- Multi value ***names***, they will not be checked.
```
names hello.com,world.com,42.com;
```

- One value ***limit***, can represent bytes, kilobytes, megabytes or gigabytes.
```
limit 1024b;
limit 1024KB;
limit 1024mB;
```

- Multi value map ***error***, represent error code and error file.
```
error 404=foo.html,500=bar.html;
```

- One value ***root***, represent the root for the error files.
```
root /tmp/foo/bar/;
```

## Route Block

The server block can have M route blocks.   
Inside of them you can have:

- Multi value ***index***, represent the index files of the route.
```
index index.html,index.cpp,index.php;
```

- One value map ***redirect***, represent the redirection of a url.
```
redirect /foo/bar/=/hello/world/;
```

- One value ***root***, represent the root of the route.
```
root /tmp/foobar/;
```

- Multi value ***methods***, represent the methods of the route, only GET, POST and/or DELETE.
```
methods DELETE,GET,POST;
```

- One value ***listing***, represent directory listing for that route.
```
listing off;
```

- One value ***path***, represent the url path of the route.
```
path /;
```

- Multi value ***cgi***, represent the active cgi of the route, only `.py` and/or `.php`.
```
cgi .php,.py;
```
