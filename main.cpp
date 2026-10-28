#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// Packet handler callback function
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    // Ethernet header length (assuming standard Ethernet)
    struct ip *ip_header = (struct ip *)(packet + 14);
    
    // Check if packet is IPv4
    if (ip_header->ip_v != 4) return;

    // Calculate IP header length
    int ip_header_len = ip_header->ip_hl * 4;
    
    // Check if protocol is TCP (protocol number 6)
    if (ip_header->ip_p != IPPROTO_TCP) return;

    struct tcphdr *tcp_header = (struct tcphdr *)(packet + 14 + ip_header_len);

    // Extract IP addresses and ports
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

    u_int16_t src_port = ntohs(tcp_header->th_sport);
    u_int16_t dst_port = ntohs(tcp_header->th_dport);

    // Calculate payload offset and size
    int tcp_header_len = tcp_header->th_off * 4;
    int total_headers_size = 14 + ip_header_len + tcp_header_len;
    int payload_len = pkthdr->len - total_headers_size;

    printf("\n[TCP Packet Captured]\n");
    printf("Source: %s:%d -> Destination: %s:%d\n", src_ip, src_port, dst_ip, dst_port);
    printf("Payload Length: %d bytes\n", payload_len);

    // Print payload if it contains data
    if (payload_len > 0) {
        const u_char *payload = packet + total_headers_size;
        printf("Payload: ");
        for (int i = 0; i < payload_len && i < 100; i++) {
            if (payload[i] >= 32 && payload[i] <= 126) {
                printf("%c", payload[i]);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    char *dev;
    struct bpf_program fp;
    char filter_exp[] = "tcp"; // Filter for TCP traffic only
    bpf_u_int32 mask;
    bpf_u_int32 net;

    // Find the default network device
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return 2;
    }
    printf("Listening on device: %s\n", dev);

    // Get network IP and mask
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    // Open device for live capture
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }

    // Compile and apply the filter
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }

    // Start capturing packets (loop indefinitely)
    pcap_loop(handle, 0, packet_handler, NULL);

    // Cleanup
    pcap_freecode(&fp);
    pcap_close(handle);
    return 0;
}