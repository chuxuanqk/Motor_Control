import sys
from PyQt5.QtWidgets import QApplication


from Stamper2.Welcome.Welcome import MainForm
from Stamper2.Preview.Preview import Preview_Form
from Stamper2.Wait_Form.Wait_Form import Seal_Form

if __name__ == '__main__':
    app = QApplication(sys.argv)
    win = MainForm()
    # win = Preview_Form()
    win.show()
    sys.exit(app.exec())
