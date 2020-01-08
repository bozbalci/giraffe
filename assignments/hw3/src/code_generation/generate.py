import jinja2


if __name__ == '__main__':
    loader = jinja2.FileSystemLoader(searchpath='./')
    environment = jinja2.Environment(loader=loader)
    path = 'flat.cpp'
    template = environment.get_template(path)
    print(template.render())