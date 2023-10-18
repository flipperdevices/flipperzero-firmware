<template>
  <div
    class="flex justify-center items-center w-full h-full bg-gray-200 overflow-hidden pattern"
  >
    <div
      class="flex flex-row h-auto justify-between w-full md:w-1/2 bg-white rounded-md shadow-xl border border-gray-300 mx-4"
    >
      <Card ref="container">
        <template v-slot:header> Configure Telegram </template>
        <template v-slot:body>
          <div class="mb-2">
            <div>SSID</div>
            <Select v-model="ssid" placeholder="SSID Name">
              <option value="">SELECT SSID</option>
              <option
                v-for="network in networks"
                :key="network"
                :value="network.ssid"
              >
                {{ network.ssid }} ({{ network.encryptionType }})
                {{ network.signal }}dB #{{ network.channel }}
              </option>
            </Select>
          </div>
          <div class="mb-2">
            <div>Password</div>
            <Input v-model="password" placeholder="SSID Password" />
          </div>
          <div>
            <div>Telegram Token</div>
            <Input
              v-model="token"
              placeholder="Ej.: 0000000000:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            />
          </div>
        </template>
        <template v-slot:footer>
          <Button @click="saveConfig">Save</Button>
        </template>
      </Card>
    </div>
  </div>
</template>
<script>
import Button from "./components/Button.vue";
import Input from "./components/Input.vue";
import Select from "./components/Select.vue";
import Card from "./components/Card.vue";
export default {
  components: {
    Button,
    Input,
    Select,
    Card,
  },
  data() {
    return {
      ssid: "",
      password: "",
      token: "",
      networks: [ ],
    };
  },
  mounted() {
    this.loadNetworks();
  },
  methods: {
    loadNetworks() {
      fetch("/networks")
        .then((response) => response.json())
        .then((response) => {
          this.networks = response.networks;
        });
    },
    verifyTelegramToken(token) {
      return /[0-9]{9}:[a-zA-Z0-9_-]{35}/.test(token);
    },
    saveConfig() {
      if (this.ssid.length == 0) {
        alert("No SSID");
        return;
      }
      if (this.password.length == 0) {
        alert("No password");
        return;
      }
      if (!this.verifyTelegramToken(this.token)) {
        alert("Token not valid!");
        return;
      }
      fetch("/save-config", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          ssid: this.ssid,
          password: this.password,
          token: this.token,
        }),
      })
        .then(() => {
          alert("Saved Config!");
        })
        .catch(() => {
          alert("Internal Error!");
        });
    },
  },
};
</script>
<style lang="css"></style>
