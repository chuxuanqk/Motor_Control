import sys
from PyQt5.QtWidgets import QApplication


from Stamper2.Welcome.Welcome import MainForm


if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = MainForm()
    win.show_self()
    sys.exit(app.exec())
