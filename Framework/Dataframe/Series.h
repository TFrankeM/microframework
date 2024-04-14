#include <vector>
#include <string>

using std::string;
using std::vector;

namespace framework
{
    namespace dataframe
    {
        template <typename T>
        class Series
        {
        private:
            vector<T> data;
        public:
            Series() = default;
            Series(const vector<T> &data);
            Series(const Series<T> &other);
            ~Series() = default;

            T &operator[](size_t index);
            const T &operator[](size_t index) const;
            int size() const { return data.size(); }
        };
    }
}