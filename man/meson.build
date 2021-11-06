man_en = configure_file(input: 'katoob.1.in',
                        output: 'katoob.1',
                        configuration: conf)

man_de = configure_file(input: 'katoob.de.1.in',
                        output: 'katoob.de.1',
                        configuration: conf)

install_man(man_en)
install_man(man_de, locale: 'de')
