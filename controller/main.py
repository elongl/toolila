from controller import Controller


def main():
    controller = Controller('127.0.0.1')
    controller.connect()
    print(controller.shell('whoami'))


if __name__ == "__main__":
    main()
