import pathlib

import jinja2

DIR_PATH = pathlib.Path(__file__).parent


def render_template(*, in_file, out_file, context):
    with open(in_file, "r") as in_f:
        template = jinja2.Template(in_f.read())

    with open(out_file, "w") as out_f:
        out_f.write(template.render(**context))


if __name__ == "__main__":
    render_template(
        in_file=DIR_PATH / "send_array.proto.in",
        out_file=DIR_PATH / "send_array.proto",
        context=dict(
            data_types=[
                "double",
                "float",
                "int32",
                "int64",
                # "uint32",
                # "uint64",
                "sint32",
                "sint64",
                # "fixed32",
                # "fixed64",
                "sfixed32",
                "sfixed64",
            ]
        ),
    )
