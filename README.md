# cpp-cloudflare-scraper

Use this to get the cookies required for cloudflare.

```cpp
int main(int argc, char *argv[]) {

    jsResponse rs;
    cpr::Response r = rs.get("cloudflare-site-example.com");
    cpr::Cookies cfCookies = r.cookies;
    
    //use cfCookies for scraping the site.

    return 0;
}
```

# Dependencies

* [cpr](https://github.com/whoshuu/cpr)
* [duktape](https://github.com/svaarala/duktape)
