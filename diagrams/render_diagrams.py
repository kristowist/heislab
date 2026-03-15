import requests
import pathlib

KROKI_URL = "https://kroki.io/plantuml/png"

def render(puml_path: str):
    path = pathlib.Path(puml_path)
    text = path.read_text(encoding="utf-8")
    response = requests.post(
        KROKI_URL,
        data=text.encode("utf-8"),
        headers={"Content-Type": "text/plain"},
    )
    response.raise_for_status()
    out_path = path.with_suffix(".png")
    out_path.write_bytes(response.content)
    print(f"Lagret: {out_path}")

if __name__ == "__main__":
    render("state_diagram.puml")
    render("sequence_diagram.puml")
    render("class_diagram.puml")
