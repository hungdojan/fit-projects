import re
import requests
import sys
from bs4 import BeautifulSoup
from lxml import etree


class ProductItem:
    def __init__(self, name, price, *param):
        self.name = name
        self.price = price
        self.params = list(param)

    def __str__(self):
        def _get_values():
            # list of nutrition categories
            nutrition_header = CSV_HEADER[5:]
            # map of nutrition values
            _nutritions = self.params[-1]
            return [
                _nutritions[i] if _nutritions.get(i) else "" for i in nutrition_header
            ]

        # convert map of nutrition values to list of nutrition values
        values = _get_values()
        self.params.pop()
        self.params += values

        # create a final string
        params = "\t".join(self.params)
        return f"{self.name}\t{self.price}\t{params}"


HEADERS = {
    "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36"
}
XPATH_MAP = {
    "info": '//div[@class="right fw floatn margint_medium-1099 paddedt_medium-1099 bordert_1-1099"]',
    "name": "/h1",
    "price": '/*[@class="price"]',
    "details": '/*[@class="details"]',
    "brand": '//h6[text()="Brand"]/../p/a',
    "package_size": '/div[contains(@class, "packsize")]/p',
    "product_of": '/div[contains(@class, "productof")]/p',
    "nutrition": '/div[contains(@class, "nutrition")]/table',
}

CSV_HEADER = ["Name", "Price", "Brand", "Size", "Country of origin"]
NUTRITION_HEADER = []


def el_find(dom, *args):
    """Search for an element using XPath.

    Always returns the first instance if found.

    Args:
        dom (lxml.HTML): Root DOM object.
        *args (list[str]): XPath strings to find an element.

    Returns:
        (lxml._Element|None): The element if found; None otherwise.
    """
    el = dom.xpath("".join(args))
    if el:
        return el[0]
    return None


def regex_number(_s):
    """Parse integer using regex.

    Extract an integer value, prefix and sufix of the given string.

    Args:
        _s (str): Given string.

    Returns:
        (tuple[str, str, str]): Prefix, integer data and sufix.
    """
    search = re.search(r"(\D*)(\d+\.?\d*)(\D*)", _s)
    if search:
        prefix = search.group(1)
        data = search.group(2)
        sufix = search.group(3)
        return prefix, data, sufix
    return "", "", ""


def get_text(dom, *args):
    """Get text from the element.

    Args:
        dom (lxml.HTML): Root DOM object.
        *args (list[str]): XPath strings to find an element.

    Returns:
        (str): Element's text if found; empty string otherwise.
    """
    el = el_find(dom, *args)
    if el is None:
        return ""
    return el.text


def main():
    """Main function."""

    lof_production = []
    s = requests.session()
    for index, line in enumerate(sys.stdin):
        ret = s.get(url=line.strip(), headers=HEADERS)
        bs = BeautifulSoup(ret.text, "html.parser")
        dom = etree.HTML(str(bs))
        args = []
        nutrition = {}

        # product name
        name_text = get_text(dom, XPATH_MAP["info"], XPATH_MAP["name"])
        # product price
        price_text = get_text(dom, XPATH_MAP["info"], XPATH_MAP["price"], "/em")
        pre, price_data, _ = regex_number(price_text)
        if index == 0:
            CSV_HEADER[1] = f"{CSV_HEADER[1]} ({pre})"
        # product brand
        brand_text = get_text(
            dom, XPATH_MAP["info"], XPATH_MAP["details"], XPATH_MAP["brand"]
        )
        # product package size
        packsize_text = get_text(
            dom, XPATH_MAP["info"], XPATH_MAP["details"], XPATH_MAP["package_size"]
        )
        _, packsize_data, suf = regex_number(packsize_text)
        if index == 0:
            CSV_HEADER[3] = f"{CSV_HEADER[3]} ({suf})"
        # product's country of origin
        productof_text = get_text(
            dom, XPATH_MAP["info"], XPATH_MAP["details"], XPATH_MAP["product_of"]
        )
        args.append(brand_text)
        args.append(packsize_data)
        args.append(productof_text)
        # retrieve data from the nutrition table
        nutrition_table_element = el_find(
            dom, XPATH_MAP["info"], XPATH_MAP["details"], XPATH_MAP["nutrition"]
        )
        if nutrition_table_element is not None:
            for i in nutrition_table_element.getchildren():
                # nutrition category
                col_name = i.getchildren()[0].text
                # nutrition value
                col_val = i.getchildren()[1].getchildren()[0].getchildren()[0].text
                _, col_data, suf = regex_number(col_val)
                # create new header column if not exists
                if not col_name in NUTRITION_HEADER:
                    CSV_HEADER.append(f"{col_name} ({suf})")
                    NUTRITION_HEADER.append(col_name)
                nutrition[f"{col_name} ({suf})"] = col_data
        args.append(nutrition)

        # push item to the list
        pitem = ProductItem(
            name_text,
            price_data,
            *args,
        )
        lof_production.append(pitem)

    # print data in TSV format to STDOUT
    print("\t".join(CSV_HEADER))
    for i in lof_production:
        print(str(i))


if __name__ == "__main__":
    main()
