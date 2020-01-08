import jinja2
import sys


if __name__ == '__main__':
    loader = jinja2.FileSystemLoader(searchpath='../templates')
    environment = jinja2.Environment(loader=loader)
    path = sys.argv[1]
    template = environment.get_template(path)
    print(template.render())
