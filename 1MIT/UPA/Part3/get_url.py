import requests
from bs4 import BeautifulSoup
from lxml import etree

headers = {
    "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36"
}
root_url = "https://www.orientalmart.co.uk/"
list_xpath = (
    '//ul[@class="item-list flex falignst product productnotcategory layout-grid"]'
)

s = requests.session()
# iterate through the pages
# each page contains 20 products therefore we need to open 5 pages
for i in range(1, 6):
    # retrieve the list
    ret = s.get(f"{root_url}snacks-and-sweets?Page={i}", headers=headers)
    # get DOM parser
    bs = BeautifulSoup(ret.text, "html.parser")
    dom = etree.HTML(str(bs))
    # find the element containing the list of products on the selected page
    list_element = dom.xpath(list_xpath)[0].getchildren()

    for item in list_element:
        # get h3 element
        h3_element = item.getchildren()[2]
        a_element = h3_element.getchildren()[0]
        # print(a_element.text, a_element.get("href"))
        print(f"{root_url}{a_element.get('href')}")
