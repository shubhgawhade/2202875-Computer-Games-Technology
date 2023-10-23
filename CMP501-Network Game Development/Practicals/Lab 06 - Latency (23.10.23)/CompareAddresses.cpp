// Comparison operators for sockaddr_in.
// These definitions will be useful if you want to use the standard library
// collections with sockaddr_in, e.g. if you want to create something like:
//   std::set<sockaddr_in> something;
//   std::map<sockaddr_in, Player> something;

bool operator==(const sockaddr_in& left, const sockaddr_in& right)
{
    return (left.sin_port == right.sin_port)
        && (memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr)) == 0);
}

bool operator<(const sockaddr_in& left, const sockaddr_in& right)
{
    int c = memcmp(&left.sin_addr, &right.sin_addr, sizeof(left.sin_addr));
    if (c < 0)
        return true;
    else if (c == 0)
        return left.sin_port < right.sin_port;
    else
        return false;
}