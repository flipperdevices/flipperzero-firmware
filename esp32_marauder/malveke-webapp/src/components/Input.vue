<template>
    <input  v-model="value" autocomplete="false" type="text" class="text-xs bg-gray-50 border border-gray-300 text-gray-900 text-sm rounded-lg focus:ring-blue-500 focus:border-blue-500 block w-full p-2.5 dark:bg-gray-700 dark:border-gray-600 dark:placeholder-gray-400 dark:text-white dark:focus:ring-blue-500 dark:focus:border-blue-500" @input="$emit('update:modelValue', $event.target.value)" />
</template>
<script>
export default {
    props: {
        modelValue: String,
    },
    data() {
        return {
            value: this.modelValue
        }
    }
}
</script>
<style lang="">
    
</style>