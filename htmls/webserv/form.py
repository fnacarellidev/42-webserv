import sys

if (len(sys.argv) < 2):
    print("No parameters received on our cgi :(")
    sys.exit(1)

tup = []
pairs = sys.argv[1].split('&')

for pair in pairs:
    key, val = pair.split('=')
    tup.append((key, val))

print("<!DOCTYPE html> \n<html lang=\"en\"> \n<head> \n<meta charset=\"UTF-8\"> \n<title>Python Form</title> \n</head>\n<body>\n")
for (key, val) in tup:
    print("<h2>", key, ": ", val, "</h2>", sep="")
print("\n</body>\n</html>")
